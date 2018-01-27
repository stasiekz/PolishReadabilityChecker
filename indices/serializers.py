from django.conf import settings
from django.core.files.base import ContentFile
from rest_framework import serializers
from rest_framework.exceptions import ValidationError
from rest_framework.reverse import reverse

from indices.models import Analysis
from indices.utils import md5
from indices.indices import Readability

from indices.exceptions import InvalidFileFormatError


class AnalysisSerializer(serializers.ModelSerializer):
    html_text_url = serializers.SerializerMethodField()
    text = serializers.CharField(allow_blank=True, required=False, write_only=True)
    file = serializers.FileField(allow_null=True, required=False, write_only=True)

    class Meta:
        model = Analysis
        fields = ('text', 'file', 'html_text_url', 'stats')
        extra_kwargs = {
            'stats': {'read_only': True}
        }

    def get_html_text_url(self, obj):
        return reverse('analyses-text', kwargs={'md5_checksum': obj.md5_checksum}, request=self.context.get('request'))

    def validate(self, attrs):
        if not any([attrs.get('text'), attrs.get('file')]):
            raise serializers.ValidationError('Please provide either text or file field.')
        return attrs

    def create(self, validated_data):
        text, file = validated_data.pop('text', None), validated_data.pop('file', None)
        md5_checksum = md5(text or file)
        obj = Analysis.objects.filter(md5_checksum=md5_checksum).first()
        if obj:
            return obj
        readability = Readability(text or file)
        try:
            readability.compute_indices()
        except InvalidFileFormatError:
            raise ValidationError("Invalid file format, please make sure it is one of the following: {}".format(
                settings.ACCEPTED_FILE_EXTENSIONS))
        obj = Analysis(md5_checksum=md5_checksum, stats=readability.data, html_text=readability.html_text)
        if text:
            obj.file.save("{}.txt".format(md5_checksum), ContentFile(text))
        else:
            obj.file.save(file.name, file)

        return obj
